/*
  - TODO(@cobileacd): refactor
*/

import { IonBackdrop, IonBadge, IonButton, IonButtons, IonCard, IonCardContent, IonCardHeader, IonCardSubtitle, IonCardTitle, IonChip, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonInput, IonItem, IonItemDivider, IonLabel, IonModal, IonNote, IonPage, IonPopover, IonRouterOutlet, IonRow, IonSearchbar, IonSelect, IonSelectOption, IonText, IonTextarea, IonTitle, IonToolbar, SearchbarInputEventDetail } from "@ionic/react";
import ApiWrapper from "../support/APIWrapper";
import { Route, useHistory, useParams } from "react-router-dom";
import { useEffect, useRef, useState } from "react";
import { arrowBack, backspace, enterOutline, eyeOutline, helpCircle, returnDownBack, searchOutline, warning } from "ionicons/icons";
import CreateVaultContainer from "../components/CreateVaultContainer";
import "../support/General.css"
import "./Organizations.css"
import AddMemberContainer from "../components/AddMemberContainer";
import Common from "../support/Common";
import { format } from "date-fns";
import AppAppBar from "../components/AppAppBar";
import CreateVault from "../components/CreateVault";
import { IonSearchbarCustomEvent } from "@ionic/core";

const Organization: React.FC = () => {
    const { id: organizationId } = useParams<{ id: string }>();
    const [vaults, setVaults] = useState<any[]>([]);
    const [members, setMembers] = useState<any[]>([]);
    const [role, setRole] = useState<any>(null);
    const [newRole, setNewRole] = useState<string>("");
    const [permissions, setPermissions] = useState<any[]>([]);
    const [organization, setOrganization] = useState<any>(null);
    const [title, setTitle] = useState<string>("");
    const history = useHistory();

    const modal = useRef<HTMLIonModalElement>(null);
    const input = useRef<HTMLIonInputElement>(null);

    useEffect(() => {
        fetchOrganization();
    }, []);

    const fetchOrganization = async () => {
        try {
            const response = await ApiWrapper.fetchOrganizationDetails(organizationId);
            if (response) {
                setMembers(response.data.members);
                setVaults(response.data.vaults);
                setResults(response.data.vaults);
                setRole(response.data.role);
                setPermissions(response.data.permissions);
                setOrganization(response.data.organization);
                setTitle(response.data.organization.name);
            } else {
                setVaults([]);
                setResults([]);
                setMembers([]);
                setRole("");
                setPermissions([]);
                setOrganization(null);
                setTitle("");
            }
        } catch (error) {
            console.error('Error fetching Organization Details', error);
        }
    };

    const createRole = () => {
        const formData = new URLSearchParams();
        formData.append('role', newRole);
        formData.append('organization', organizationId);
        ApiWrapper.createRole(formData);
    };

    const handleGoBack = () => {
        history.goBack();
    };

    const [name, setName] = useState('');
    const [description, setDescription] = useState('');
    const [view, setView] = useState(Common.DEFAULT_ROLES.EVERYONE);
    const [edit, setEdit] = useState(Common.DEFAULT_ROLES.ADMIN);
    const [manage, setManage] = useState(Common.DEFAULT_ROLES.ADMIN);

    const handleSubmit = () => {
        const formData = new URLSearchParams();
        formData.append('name', name);
        formData.append('description', description);
        formData.append('VIEW', view);
        formData.append('EDIT', edit);
        formData.append('MANAGE', manage);
        formData.append('organization', organizationId)
        console.log({ name, description, view, edit, manage, organizationId });
        ApiWrapper.createVault(formData).then(response => fetchOrganization());
    };

    const validateForm = () => {
        return name.trim().length > 0 && description.trim().length > 0;
    };

    const [results, setResults] = useState<any[]>([]);
    const [currentPage, setCurrentPage] = useState(1);
    const [itemsPerPage] = useState(10);

    function confirm() {
        if (validateForm()) {
            handleSubmit();
            modal.current?.dismiss(input.current?.value, 'confirm');
        }
    }

    const [username, setUsername] = useState('');
    const [formValid, setFormValid] = useState(false);
    const [inviteStatus, setInviteStatus] = useState("");

    const handleInviteSubmit = async () => {
        const formData = new URLSearchParams();
        formData.append('username', username);
        console.log(organization);
        formData.append('organization', organization.id);
        const response = await ApiWrapper.inviteMember(formData)
        if (response instanceof Error) {
            setInviteStatus("Invalid Username.")
        }
        else {
            setInviteStatus("Invite sent successfully!")
        }
    };

    const validateInviteForm = () => {
        return username.trim().length > 0;
    };

    const invitemodal = useRef<HTMLIonModalElement>(null);

    function invite() {
        if (validateInviteForm()) {
            handleInviteSubmit();
            invitemodal.current?.dismiss(null, 'confirm');
        }
    }

    const rolemodal = useRef<HTMLIonModalElement>(null);

    function confirmRole() {
        if (newRole.length != 0) {
            createRole();
            rolemodal.current?.dismiss(null, 'confirm');
        }
    }

    const handleInput = (ev: Event) => {
        const target = ev.target as HTMLIonSearchbarElement;
        const query = target ? target.value!.toLowerCase() : '';
        setResults(vaults.filter((d) => d.vault.name.toLowerCase().indexOf(query) > -1));
        setCurrentPage(1);
    };

    const handlePageChange = (newPage: number) => {
        setCurrentPage(newPage);
    };

    const paginatedResults = results.slice((currentPage - 1) * itemsPerPage, currentPage * itemsPerPage);

    const renderPagination = () => {
        const totalPages = Math.ceil(results.length / itemsPerPage);
        const pages = [];
        for (let i = 1; i <= totalPages; i++) {
            pages.push(
                <a key={i} className={i == currentPage ? 'active' : ''} onClick={() => handlePageChange(i)}>{i}</a>
            );
        }
        return pages;
    };

    function handleMemberInput(ev: IonSearchbarCustomEvent<SearchbarInputEventDetail>): void {
        throw new Error("Function not implemented.");
    }

    return (
        <IonPage>
            <IonHeader>
                <AppAppBar title={"Organization/" + title} />
            </IonHeader>
            <IonContent className="ion-padding">
                <IonGrid className='grid'>
                    <IonRow>
                        <IonCol size="auto">
                            <IonButton className="return_button" onClick={() => handleGoBack()} fill="outline" size="small" color={"secondary"}>
                                <IonIcon icon={arrowBack} size="large"></IonIcon>
                                Return
                            </IonButton>
                        </IonCol>
                    </IonRow>
                    <IonRow>
                        <IonCol>
                            <IonCard>
                                <IonCardContent>
                                    <IonGrid>
                                        <IonTitle>Vaults</IonTitle>
                                        <IonRow>
                                            <IonCol>
                                                <IonSearchbar mode='ios' animated={true} placeholder='Search for a specific vault...' onIonInput={(ev) => handleInput(ev)}></IonSearchbar>
                                            </IonCol>
                                            {permissions.includes(Common.PERMISSIONS.CREATE_VAULTS) ? (
                                                <>
                                                    <IonCol size='auto'>
                                                        <IonButton id='open-modal' className='create-org' fill='outline' shape='round' color='success'>Add vault</IonButton>
                                                    </IonCol>
                                                    <IonModal ref={modal} trigger="open-modal">
                                                        <IonHeader>
                                                            <IonToolbar mode='ios'>
                                                                <IonButtons slot="start">
                                                                    <IonButton onClick={() => modal.current?.dismiss()}>Cancel</IonButton>
                                                                </IonButtons>
                                                                <IonTitle>Create Vault</IonTitle>
                                                                <IonButtons slot="end">
                                                                    <IonButton strong={true} onClick={() => confirm()}>
                                                                        Confirm
                                                                    </IonButton>
                                                                </IonButtons>
                                                            </IonToolbar>
                                                        </IonHeader>
                                                        <IonContent className="ion-padding">
                                                            <IonItem><IonLabel color={'primary'}>Definition</IonLabel></IonItem>
                                                            <IonItem>
                                                                <IonInput
                                                                    label="Vault&#39;s name"
                                                                    type="text"
                                                                    value={name}
                                                                    labelPlacement="stacked"
                                                                    onIonChange={(e) => {
                                                                        setName(e.detail.value!);
                                                                    }}
                                                                />
                                                            </IonItem>
                                                            <IonItem>
                                                                <IonTextarea
                                                                    label="Short description"
                                                                    labelPlacement="stacked"
                                                                    value={description}
                                                                    onIonChange={(e) => {
                                                                        setDescription(e.detail.value!);
                                                                    }}
                                                                />
                                                            </IonItem>
                                                            <IonItem className='ion-padding-top'>
                                                                <IonLabel color='primary'>Permissions<IonIcon icon={helpCircle} size='medium'></IonIcon></IonLabel>
                                                            </IonItem>
                                                            <IonItem>
                                                                <IonSelect label='View' labelPlacement="stacked" interface='popover' onIonChange={(e) => { setView(e.detail.value) }} value={Common.DEFAULT_ROLES.EVERYONE}>
                                                                    <IonSelectOption disabled={true}>
                                                                        <IonLabel style={{ color: '#808080', fontStyle: 'italic' }}>--Default Roles--</IonLabel>
                                                                    </IonSelectOption>
                                                                    <IonSelectOption value={Common.DEFAULT_ROLES.EVERYONE}>Everyone</IonSelectOption>
                                                                    <IonSelectOption value={Common.DEFAULT_ROLES.ADMIN}>Admins</IonSelectOption>
                                                                    <IonSelectOption value={Common.DEFAULT_ROLES.ME}>Just Me</IonSelectOption>
                                                                    <IonSelectOption disabled={true}>
                                                                        <IonLabel style={{ color: '#808080', fontStyle: 'italic' }}>--Personalized Roles--</IonLabel>
                                                                    </IonSelectOption>
                                                                </IonSelect>
                                                            </IonItem>

                                                            <IonItem>
                                                                <IonSelect label='Edit' labelPlacement="stacked" interface='popover' onIonChange={(e) => { setEdit(e.detail.value) }} value={Common.DEFAULT_ROLES.ADMIN}>
                                                                    <IonSelectOption disabled={true}>
                                                                        <IonLabel style={{ color: '#808080', fontStyle: 'italic' }}>--Default Roles--</IonLabel>
                                                                    </IonSelectOption>
                                                                    <IonSelectOption value={Common.DEFAULT_ROLES.EVERYONE}>Everyone</IonSelectOption>
                                                                    <IonSelectOption value={Common.DEFAULT_ROLES.ADMIN}>Admins</IonSelectOption>
                                                                    <IonSelectOption value={Common.DEFAULT_ROLES.ME}>Just Me</IonSelectOption>
                                                                    <IonSelectOption disabled={true}>
                                                                        <IonLabel style={{ color: '#808080', fontStyle: 'italic' }}>--Personalized Roles--</IonLabel>
                                                                    </IonSelectOption>
                                                                </IonSelect>
                                                            </IonItem>
                                                            <IonItem>
                                                                <IonSelect label='Manage' labelPlacement='stacked' interface='popover' onIonChange={(e) => { setManage(e.detail.value) }} value={Common.DEFAULT_ROLES.ADMIN}>
                                                                    <IonSelectOption disabled={true}>
                                                                        <IonLabel style={{ color: '#808080', fontStyle: 'italic' }}>--Default Roles--</IonLabel>
                                                                    </IonSelectOption>
                                                                    <IonSelectOption value={Common.DEFAULT_ROLES.EVERYONE}>Everyone</IonSelectOption>
                                                                    <IonSelectOption value={Common.DEFAULT_ROLES.ADMIN}>Admins</IonSelectOption>
                                                                    <IonSelectOption value={Common.DEFAULT_ROLES.ME}>Just Me</IonSelectOption>
                                                                    <IonSelectOption disabled={true}>
                                                                        <IonLabel style={{ color: '#808080', fontStyle: 'italic' }}>--Personalized Roles--</IonLabel>
                                                                    </IonSelectOption>
                                                                </IonSelect>
                                                            </IonItem>
                                                        </IonContent>
                                                    </IonModal>
                                                </>
                                            ) : (<></>)}
                                        </IonRow>
                                        <IonGrid>
                                            <IonRow>
                                                <IonCol><IonLabel><h2>Name</h2></IonLabel></IonCol>
                                                <IonCol><IonLabel><h2>Description</h2></IonLabel></IonCol>
                                                <IonCol></IonCol>
                                            </IonRow>
                                            <IonItemDivider></IonItemDivider>
                                            {paginatedResults.length !== 0 ? (
                                                paginatedResults.map((item, index) => (
                                                    <IonRow key={item.vault.id}>
                                                        <IonCol className="appt_col">
                                                            <IonLabel>{item.vault.name}</IonLabel>
                                                        </IonCol>
                                                        <IonCol className="appt_col">
                                                            <IonLabel>{item.vault.description}</IonLabel>
                                                        </IonCol>
                                                        <IonCol>
                                                            <IonButton shape="round" fill="outline" className="appt_button" href={"/organization/vault/" + item.vault.id}>Access<IonIcon icon={enterOutline}></IonIcon></IonButton>
                                                        </IonCol>
                                                        <IonItemDivider></IonItemDivider>
                                                    </IonRow>
                                                ))
                                            ) : (
                                                <IonText>No vaults found.</IonText>
                                            )}
                                        </IonGrid>
                                        <IonRow>
                                            <div className="pagination ion-margin-top">
                                                <a onClick={() => { currentPage > 1 ? setCurrentPage(currentPage - 1) : false }}>&laquo;</a>
                                                {renderPagination()}
                                                <a onClick={() => { currentPage < Math.ceil(results.length / itemsPerPage) ? setCurrentPage(currentPage + 1) : false }}>&raquo;</a>
                                            </div>
                                        </IonRow>
                                    </IonGrid>
                                </IonCardContent>
                            </IonCard>
                        </IonCol>
                        <IonCol size="auto">
                            {organization ? (
                                <IonCard className="org-card">
                                    <IonCardHeader>
                                        <IonCardTitle>
                                            <IonLabel>{organization.name}</IonLabel>
                                        </IonCardTitle>
                                        <IonCardSubtitle>
                                            <IonLabel>{organization.description}</IonLabel>
                                        </IonCardSubtitle>
                                    </IonCardHeader>
                                    <IonCardContent>
                                        <IonChip color={role.role === Common.DEFAULT_ROLES.OWNER ? ("tertiary") : (role.role === Common.DEFAULT_ROLES.ADMIN ? ("warning") : ("success"))}>{role.role}</IonChip>
                                    </IonCardContent>
                                    {permissions.includes(Common.PERMISSIONS.MANAGE_ORGANIZATION) ? (
                                        <>
                                            <IonButton fill="clear" id="add-role">Add Role</IonButton>
                                            <IonModal ref={rolemodal} trigger="add-role">
                                                <IonHeader>
                                                    <IonToolbar mode='ios'>
                                                        <IonButtons slot="start">
                                                            <IonButton onClick={() => rolemodal.current?.dismiss()}>Cancel</IonButton>
                                                        </IonButtons>
                                                        <IonTitle>Add Role</IonTitle>
                                                        <IonButtons slot="end">
                                                            <IonButton strong={true} onClick={() => confirmRole()}>
                                                                Confirm
                                                            </IonButton>
                                                        </IonButtons>
                                                    </IonToolbar>
                                                </IonHeader>
                                                <IonContent className="ion-padding">
                                                    <IonInput
                                                        type="text"
                                                        label="Role Name"
                                                        labelPlacement="stacked"
                                                        value={newRole}
                                                        onIonChange={(e) => {
                                                            setNewRole(e.detail.value!);
                                                        }}
                                                    />
                                                    <IonItemDivider/>
                                                </IonContent>
                                            </IonModal>
                                        </>
                                    ) : (<></>)}
                                </IonCard>
                            ) : (
                                <></>
                            )}
                        </IonCol>
                    </IonRow>
                    <IonRow>
                        <IonCol>
                            <IonCard>
                                <IonCardContent>
                                    <IonGrid>
                                        <IonTitle>Members</IonTitle>
                                        <IonRow>
                                            <IonCol>
                                                <IonSearchbar mode='ios' animated={true} placeholder='Search for a specific member...' onIonInput={(ev) => handleMemberInput(ev)}></IonSearchbar>
                                            </IonCol>
                                            {permissions.includes(Common.PERMISSIONS.MANAGE_MEMBERS) ? (
                                                <IonCol size='auto'>
                                                    <IonButton id='invite-member' className='create-org' fill='outline' shape='round' color='success'>Add Member</IonButton>
                                                    <IonModal ref={invitemodal} trigger="invite-member">
                                                        <IonHeader>
                                                            <IonToolbar mode='ios'>
                                                                <IonButtons slot="start">
                                                                    <IonButton onClick={() => invitemodal.current?.dismiss()}>Cancel</IonButton>
                                                                </IonButtons>
                                                                <IonTitle>Invite Member</IonTitle>
                                                                <IonButtons slot="end">
                                                                    <IonButton strong={true} onClick={() => invite()}>
                                                                        Invite
                                                                    </IonButton>
                                                                </IonButtons>
                                                            </IonToolbar>
                                                        </IonHeader>
                                                        <IonContent className="ion-padding">
                                                            <IonInput
                                                                type="text"
                                                                label="Username"
                                                                labelPlacement="stacked"
                                                                placeholder="Username"
                                                                value={username}
                                                                onIonChange={(e) => {
                                                                    setUsername(e.detail.value!);
                                                                }}
                                                            />
                                                            <IonItemDivider />
                                                        </IonContent>
                                                    </IonModal>
                                                </IonCol>
                                            ) : (<></>)}
                                        </IonRow>
                                        <IonRow>
                                            <IonCol><IonLabel><h2>Name</h2></IonLabel></IonCol>
                                            <IonCol><IonLabel><h2>Role</h2></IonLabel></IonCol>
                                            <IonCol><IonLabel><h2>Joined</h2></IonLabel></IonCol>
                                        </IonRow>
                                        <IonItemDivider></IonItemDivider>
                                        {members.length !== 0 ? (
                                            members.map((item, index) => (
                                                <div key={item.username}>
                                                    <IonRow>
                                                        <IonCol className="appt_col">
                                                            <IonLabel>{item.name}</IonLabel>
                                                        </IonCol>
                                                        <IonCol className="appt_col">
                                                            <IonChip color={item.role.role === Common.DEFAULT_ROLES.OWNER ? ("tertiary") : (item.role.role === Common.DEFAULT_ROLES.ADMIN ? ("warning") : ("success"))}>{item.role.role}</IonChip>
                                                        </IonCol>
                                                        <IonCol className="appt_col">
                                                            <IonLabel>{format(new Date(item.joined), "dd-MM-yyyy")}</IonLabel>
                                                        </IonCol>
                                                    </IonRow>
                                                    <IonItemDivider />
                                                </div>
                                            ))
                                        ) : (
                                            <IonTitle className='ion-padding-vertical'>No members yet.</IonTitle>
                                        )}
                                    </IonGrid>
                                </IonCardContent>
                            </IonCard>
                        </IonCol>
                    </IonRow>
                    <IonRow>
                    </IonRow>
                </IonGrid>
            </IonContent>
        </IonPage >
    );
};

export default Organization;
