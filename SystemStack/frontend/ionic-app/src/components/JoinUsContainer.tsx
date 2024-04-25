import { useContext } from 'react';
import { URIContext } from '../App';
import './ExploreContainer.css';

const JoinUsContainer: React.FC = () => {
  const uri = useContext(URIContext)
  return (
    <div className="container">
      <strong>Since 2024</strong>
      <p>Value your privacy? <a href={uri + "login"}>Join Us.</a></p>
    </div>
  );
};

export default JoinUsContainer;
